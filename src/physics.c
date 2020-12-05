#include "physics.h"

#include "math_util.h"
#include "game_state.h"
#include "globals.h"

#include <ultra64.h>
#include <gu.h>

void updateVerletBody(VerletBody* verletBody, const Physics* physics) {
    if(!verletBody->isStatic) {
        Vec2f velocity = sub2f(verletBody->position, verletBody->oldPosition);
        addTo2f(&velocity, mul2f(physics->gravity, physics->deltaTime));
        verletBody->oldPosition = verletBody->position;
        addTo2f(&verletBody->position, velocity);
    }
}

void constrainVerletBody(VerletBody* verletBody, const Physics* physics) {
    if(!verletBody->isStatic) {
        Vec2f velocity = sub2f(verletBody->position, verletBody->oldPosition);
        if (verletBody->position.x - verletBody->radius < physics->minBoundary.x) {
            verletBody->position.x = physics->minBoundary.x + verletBody->radius;
            if (verletBody->oldPosition.x > verletBody->position.x) {
                verletBody->oldPosition.x = verletBody->position.x + velocity.x * verletBody->bounciness;
            }
        } else if (verletBody->position.x + verletBody->radius > physics->maxBoundary.x) {
            verletBody->position.x = physics->maxBoundary.x - verletBody->radius;
            if (verletBody->oldPosition.x < verletBody->position.x) {
                verletBody->oldPosition.x = verletBody->position.x + velocity.x * verletBody->bounciness;
            }
        }
        if (verletBody->position.y - verletBody->radius < physics->minBoundary.y) {
            verletBody->position.y = physics->minBoundary.y + verletBody->radius;
            if (verletBody->oldPosition.y > verletBody->position.y) {
                verletBody->oldPosition.y = verletBody->position.y + velocity.y * verletBody->bounciness;
            }
        } else if (verletBody->position.y + verletBody->radius > physics->maxBoundary.y) {
            verletBody->position.y = physics->maxBoundary.y - verletBody->radius;
            if (verletBody->oldPosition.y < verletBody->position.y) {
                verletBody->oldPosition.y = verletBody->position.y + velocity.y * verletBody->bounciness;
            }
        }
    }
}

void constrainColliders(VerletBody* verletBodyA, VerletBody* verletBodyB, const Physics* physics) {
    float minSeparation = verletBodyA->radius + verletBodyB->radius;
    Vec2f delta = sub2f(verletBodyB->position, verletBodyA->position);
    float distanceSq = dot2f(delta, delta);
    if (distanceSq < minSeparation * minSeparation) {
        float distance = sqrtf(distanceSq);
        float distanceDelta = distance - minSeparation;
        Vec2f direction = mul2f(delta, 1.0f / distance);
        Vec2f finalDelta = mul2f(direction, distanceDelta);

        Vec2f velocityA = sub2f(verletBodyA->position, verletBodyA->oldPosition);
        Vec2f velocityB = sub2f(verletBodyB->position, verletBodyB->oldPosition);

        float totalMass = !verletBodyB->isStatic * verletBodyA->mass + !verletBodyA->isStatic * verletBodyB->mass;

        float dotA = dot2f(velocityA, direction);
        float dotB = dot2f(velocityB, direction);

        if(!verletBodyA->isStatic) {
            addTo2f(&verletBodyA->position, mul2f(finalDelta, verletBodyB->mass / totalMass));
        }
        if(!verletBodyB->isStatic) {
            subTo2f(&verletBodyB->position, mul2f(finalDelta, verletBodyA->mass / totalMass));
        }

//        if (dotA > 0) {
//            addTo2f(&velocityA, mul2f(direction, -dotA * (1 + verletBodyA->bounciness)));
//        }
//        if (dotB < 0) {
//            addTo2f(&velocityB, mul2f(direction, +dotB * (1 + verletBodyB->bounciness)));
//        }
//
//        verletBodyA->oldPosition = sub2f(verletBodyA->position, velocityA);
//        verletBodyB->oldPosition = sub2f(verletBodyB->position, velocityB);
    }
}


void constrainChain(Chain* chain, const Physics* physics) {
    int i;
    for (i = 0; i < chain->nodeCount - 1; ++i) {
        VerletBody* a = &chain->nodes[i];
        VerletBody* b = &chain->nodes[i + 1];
        Vec2f positionDelta = sub2f(b->position, a->position);
        float distance = length2f(positionDelta);
        Vec2f direction = mul2f(positionDelta, 1.0f / distance);
        float distanceDelta = distance - chain->segmentLength;
        Vec2f finalDelta = mul2f(direction, distanceDelta);
        float totalMass = !b->isStatic * a->mass + !a->isStatic * b->mass;
        if(!a->isStatic) {
            addTo2f(&a->position, mul2f(finalDelta, b->mass / totalMass));
        }
        if(!b->isStatic) {
            subTo2f(&b->position, mul2f(finalDelta, a->mass / totalMass));
        }
    }
}

void updatePlayer(Player* player, const Physics* physics) {
    int i;
    int j;

    addTo2f(
        &player_getCharacter(player)->position,
        mul2f(player->movementControl, physics->deltaTime * player->movementSpeed)
    );

    for (i = 0; i < player->chain.nodeCount; ++i) {
        updateVerletBody(&player->chain.nodes[i], physics);
    }
    for (i = 0; i < physics->verletConstraintIterations; ++i) {
        constrainChain(&player->chain, physics);
        constrainColliders(player_getCharacter(player), player_getBoulder(player), physics);
        constrainVerletBody(player_getCharacter(player), physics);
        //for (j = 0; j < player->chain.nodeCount; ++j) {
        //    constrainVerletBody(&player->chain.nodes[j], physics);
        //}
    }
}

float updateDeltaTime(Physics* physics) {
    const float c_maxDeltaTime = 1.0f / 30.0f;
    float timeNow = OS_CYCLES_TO_NSEC(osGetTime()) / 1e-9f;
    physics->deltaTime = MIN(c_maxDeltaTime, timeNow - physics->timeAtLastPhysicsUpdate);
    physics->timeAtLastPhysicsUpdate = timeNow;
}

void updatePhysics(struct GameState* gameState) {
    updateDeltaTime(&gameState->physics);
    for(i = 0; i < 4; ++i) {
        if (gameState->players[i].health > 0) {
            updatePlayer(&gameState->players[i], &gameState->physics);
        }
    }
}