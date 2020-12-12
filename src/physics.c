#include "physics.h"

#include "audio.h"
#include "math_util.h"
#include "game_state.h"
#include "globals.h"

#include <ultra64.h>
#include <stdlib.h>
#include <math.h>
#include <gu.h>

void updateVerletBody(VerletBody* verletBody, const Physics* physics) {
    Vec2f velocity = sub2f(verletBody->position, verletBody->oldPosition);
    addTo2f(&velocity, mul2f(physics->gravity, physics->deltaTime));
    verletBody->oldPosition = verletBody->position;
    addTo2f(&verletBody->position, mul2f(velocity, verletBody->airResistance));
}

void collisionScreenShake(float speed, Camera* camera) {
    const float minSpeed = 250;
    const float maxScreenShake = 500;
    speed -= minSpeed;
    if(speed > 0 && camera->screenShake < maxScreenShake) {
        camera->screenShake = MIN(maxScreenShake, camera->screenShake + 0.1f * speed);
    }
}

void constrainVerletBody(VerletBody* verletBody, Camera* camera, const Physics* physics) {
    Vec2f velocity = sub2f(verletBody->position, verletBody->oldPosition);
    if (verletBody->position.x - verletBody->radius < physics->minBoundary.x) {
        verletBody->position.x = physics->minBoundary.x + verletBody->radius;
        if (verletBody->oldPosition.x > verletBody->position.x) {
            verletBody->oldPosition.x = verletBody->position.x + velocity.x * verletBody->bounciness;
            collisionScreenShake(fabs(velocity.x / physics->deltaTime), camera);
        }
    } else if (verletBody->position.x + verletBody->radius > physics->maxBoundary.x) {
        verletBody->position.x = physics->maxBoundary.x - verletBody->radius;
        if (verletBody->oldPosition.x < verletBody->position.x) {
            verletBody->oldPosition.x = verletBody->position.x + velocity.x * verletBody->bounciness;
            collisionScreenShake(fabs(velocity.x / physics->deltaTime), camera);
        }
    }
    if (verletBody->position.y - verletBody->radius < physics->minBoundary.y) {
        verletBody->position.y = physics->minBoundary.y + verletBody->radius;
        if (verletBody->oldPosition.y > verletBody->position.y) {
            verletBody->oldPosition.y = verletBody->position.y + velocity.y * verletBody->bounciness;
            collisionScreenShake(fabs(velocity.y / physics->deltaTime), camera);
        }
    } else if (verletBody->position.y + verletBody->radius > physics->maxBoundary.y) {
        verletBody->position.y = physics->maxBoundary.y - verletBody->radius;
        if (verletBody->oldPosition.y < verletBody->position.y) {
            verletBody->oldPosition.y = verletBody->position.y + velocity.y * verletBody->bounciness;
            collisionScreenShake(fabs(velocity.y / physics->deltaTime), camera);
        }
    }
}

float checkImpactSpeed(VerletBody* verletBodyA, VerletBody* verletBodyB, const Physics* physics) {
    float minSeparation = verletBodyA->radius + verletBodyB->radius;
    Vec2f delta = sub2f(verletBodyB->position, verletBodyA->position);
    float distanceSq = dot2f(delta, delta);
    if (distanceSq < minSeparation * minSeparation) {
        Vec2f velocityA = sub2f(verletBodyA->position, verletBodyA->oldPosition);
        Vec2f velocityB = sub2f(verletBodyB->position, verletBodyB->oldPosition);
        Vec2f relativeVelocity = sub2f(velocityB, velocityA);
        float distance = sqrtf(distanceSq);
        Vec2f direction = mul2f(delta, 1.0f / distance);
        float relativeSpeed = dot2f(relativeVelocity, direction);
        if(relativeSpeed > 0) {
            return 0.0f;
        } else {
            return length2f(relativeVelocity) / physics->deltaTime;
        }
    } else {
        return 0.0f;
    }
}

void updateDamagePlayerToPlayer(Player* sourcePlayer, Player* targetPlayer, float speed, GameState* gameState) {
    Camera* camera = &gameState->camera;

    if(targetPlayer->health == 0 || speed <= 0.0f) {
        return;
    }

    if(targetPlayer->invulnerabilityTimer <= 0.0f) {
        if (speed > DAMAGE_THRESHOLD_SPEED) {
//        int i;
//        for(i = 0; i < CHAIN_MAX_NODE_COUNT; ++i) {
//            targetPlayer->chain.nodes[i].mass *= PLAYER_INVULNERABILITY_MASS_MULTIPLIER;
//        }
            playRandomDrumHard();
            targetPlayer->invulnerabilityTimer = 3.0f;
            --targetPlayer->health;
            if (targetPlayer->health <= 0) {
                playRandomDeath();
            }
            if(sourcePlayer) {
                ++sourcePlayer->score;
            }
            camera->screenShake = 600;
            gameState->freezeFrame = 12;
        } else if (speed > 0.1f){
            playRandomDrumSoft();
            camera->screenShake = 200;
        }
    }
}

void updateDamageEnemyToPlayer(Player* targetPlayer, GameState* gameState) {
    return updateDamagePlayerToPlayer(NULL, targetPlayer, 9999.f, gameState);
}

void constrainColliders(VerletBody* verletBodyA, VerletBody* verletBodyB, const Physics* physics, float massMultiplierA, float* relativeSpeedOut) {
    float minSeparation = verletBodyA->radius + verletBodyB->radius;
    Vec2f delta = sub2f(verletBodyB->position, verletBodyA->position);
    float distanceSq = dot2f(delta, delta);
    if(relativeSpeedOut != NULL) {
        *relativeSpeedOut = 0.0f;
    }
    if (distanceSq < minSeparation * minSeparation) {
        float distance = sqrtf(distanceSq);
        float distanceDelta = distance - minSeparation;
        Vec2f direction = mul2f(delta, 1.0f / distance);
        Vec2f finalDelta = mul2f(direction, distanceDelta);

        Vec2f velocityA = sub2f(verletBodyA->position, verletBodyA->oldPosition);
        Vec2f velocityB = sub2f(verletBodyB->position, verletBodyB->oldPosition);

        float massA = verletBodyA->mass * massMultiplierA;
        float massB = verletBodyB->mass;
        float totalMass = massA + massB;
        float relativeSpeed = dot2f(direction, sub2f(velocityA, velocityB)) / physics->deltaTime;

        addTo2f(&verletBodyA->position, mul2f(finalDelta, massB / totalMass));
        subTo2f(&verletBodyB->position, mul2f(finalDelta, massA / totalMass));

        // Apply elastic collision if moving towards each other
        if(relativeSpeed > 0.0f) {
            const float bounciness = 0.4f;
            Vec2f common = add2f(mul2f(velocityA, massA), mul2f(velocityB, massB));
            Vec2f a = mul2f(sub2f(velocityB, velocityA), massB * bounciness);
            Vec2f b = mul2f(sub2f(velocityA, velocityB), massA * bounciness);

            Vec2f newVelocityA = mul2f(add2f(common, a), 1.0f / totalMass);
            Vec2f newVelocityB = mul2f(add2f(common, b), 1.0f / totalMass);

            verletBodyA->oldPosition = sub2f(verletBodyA->position, newVelocityA);
            verletBodyB->oldPosition = sub2f(verletBodyB->position, newVelocityB);
            if(relativeSpeedOut != NULL) {
                *relativeSpeedOut = relativeSpeed;
            }
        }
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
        float totalMass = a->mass + b->mass;
        addTo2f(&a->position, mul2f(finalDelta, b->mass / totalMass));
        subTo2f(&b->position, mul2f(finalDelta, a->mass / totalMass));
    }
}

static inline float player_getBoulderSpeed(Player* player, const Physics* physics) {
    VerletBody* boulder = player_getBoulder(player);
    return length2f(mul2f(sub2f(boulder->position, boulder->oldPosition), 1.0f / physics->deltaTime));
}

void updatePlayer(Player* player, const Physics* physics) {
    int i;

    if(!player_exists(player)) {
        return;
    }

    player->barrelSpeed = player_getBoulderSpeed(player, physics);

    // Player movement/acceleration physics.
    {
        VerletBody* character = player_getCharacter(player);
        Vec2f velocity = sub2f(character->position, character->oldPosition);
        float relativeSpeed = dot2f(player->movementControl, velocity);
        float velocityModifier = 1.0f - smoothstep(relativeSpeed, -50, 50);
        float acceleration = physics->deltaTime * player->movementSpeed * velocityModifier * 2.0f;

        // char buffer[64];
        // sprintf(buffer, "velocityModifier: %f\n", velocityModifier);
        // nuDebConCPuts(0, buffer);

        addTo2f(
            &player_getCharacter(player)->position,
            mul2f(player->movementControl, acceleration)
        );
    }

    for (i = 0; i < player->chain.nodeCount; ++i) {
        updateVerletBody(&player->chain.nodes[i], physics);
    }

    if(player->invulnerabilityTimer > 0.0f) {
        player->invulnerabilityTimer = MAX(0.0f, player->invulnerabilityTimer - physics->deltaTime);
//        if(player->invulnerabilityTimer == 0.0f) {
//            int i;
//            for(i = 0; i < CHAIN_MAX_NODE_COUNT; ++i) {
//                player->chain.nodes[i].mass /= PLAYER_INVULNERABILITY_MASS_MULTIPLIER;
//            }
//        }
    }

    // Boulder size change.
    {
        VerletBody* boulder = player_getBoulder(player);
        Chain* chain = &player->chain;
        float boulderDelta = (BIG_BOULDER_RADIUS - SMALL_BOULDER_RADIUS) * physics->deltaTime / SMALL_TO_BIG_TRANSITION_TIME;
        float chainDelta = (BIG_CHAIN_SEGMENT_LENGTH - SMALL_CHAIN_SEGMENT_LENGTH) * physics->deltaTime / SMALL_TO_BIG_TRANSITION_TIME;
        if (player->bigBoulderTimer > 0.0f) {
            player->bigBoulderTimer = MAX(0.0f, player->bigBoulderTimer - physics->deltaTime);
            boulder->radius = MIN(BIG_BOULDER_RADIUS, boulder->radius + boulderDelta);
            chain->segmentLength = MIN(BIG_CHAIN_SEGMENT_LENGTH, chain->segmentLength + chainDelta);

        } else {
            boulder->radius = MAX(SMALL_BOULDER_RADIUS, boulder->radius - boulderDelta);
            chain->segmentLength = MAX(SMALL_CHAIN_SEGMENT_LENGTH, chain->segmentLength - chainDelta);
        }
    }

    if(player->health <= 0) {
        player->despawnTimer = MAX(0.0f, player->despawnTimer - physics->deltaTime);
    }
}

void killEnemy(Enemy* enemy, GameState* gameState) {
    enemy->rotationSpeed = 720.0f * (rand() / (float)RAND_MAX - 0.5f);
    enemy->health = 0;
    enemy->despawnTimer = 3.0f;
    if(enemy->isBig) {
        gameState->camera.screenShake = 800;
        gameState->freezeFrame = 12;
    } else {
        gameState->camera.screenShake = 200;
    }
}

void updateEnemy(Enemy* enemy, GameState* gameState) {
    const Physics* physics = &gameState->physics;

    if(!enemy_exists(enemy)) {
        return;
    }

    if(enemy->health <= 0) {
        // Enemy is dead but has not despawned yet.
        enemy->rotation += enemy->rotationSpeed * physics->deltaTime;
        enemy->despawnTimer = MAX(0.0f, enemy->despawnTimer - physics->deltaTime);
        updateVerletBody(&enemy->verletBody, physics);
    } else {
        // Enemy is alive and should seek target player.
        Player* targetPlayer = &gameState->players[enemy->targetPlayerIndex];
        if (targetPlayer->health <= 0) {
            // Target player is dead.
            killEnemy(enemy, gameState);
        } else {
            // Move towards target player.
            Vec2f direction = normalize2f(
                sub2f(player_getCharacter(targetPlayer)->position, enemy->verletBody.position));
            Vec2f velocity = mul2f(direction, physics->deltaTime * enemy->movementSpeed);
            Vec2f cross = (Vec2f) {-direction.y, direction.x};
            Vec2f bob = mul2f(cross, 500.0f * physics->deltaTime * sinf(4.0f * enemy->bobTimer));
            enemy->verletBody.oldPosition = enemy->verletBody.position;
            enemy->verletBody.position = add2f(enemy->verletBody.position, add2f(velocity, bob));
            enemy->bobTimer += physics->deltaTime;
        }
    }
}

void constrainPhysics(GameState* gameState, GameMode gameMode) {
    const Physics* physics = &gameState->physics;
    int i;
    int j;

    for (i = 0; i < MAX_PLAYERS; ++i) {
        Player* player = &gameState->players[i];
        if(!player_exists(player)) continue;
        constrainChain(&player->chain, physics);
    }

    for (i = 0; i < MAX_PLAYERS; ++i) {
        Player* playerA = &gameState->players[i];
        if(!player_exists(playerA)) continue;
        constrainColliders(player_getBoulder(playerA), player_getCharacter(playerA), physics, 1.0f, NULL);
        for (j = i + 1; j < MAX_PLAYERS; ++j) {
            Player* playerB = &gameState->players[j];
            if (!player_exists(playerB)) continue;
            constrainColliders(player_getCharacter(playerA), player_getCharacter(playerB), physics, 1.0f, NULL);
            constrainColliders(player_getBoulder(playerA), player_getBoulder(playerB), physics, 1.0f, NULL);
            {
                float relativeSpeed = 0.0f;
                constrainColliders(player_getBoulder(playerA), player_getCharacter(playerB), physics,
                                   player_getBoulder(playerA)->collisionMassMultiplier, &relativeSpeed);
                if(gameMode == BATTLE) {
                    updateDamagePlayerToPlayer(playerA, playerB, relativeSpeed, gameState);
                }
            }
            {
                float relativeSpeed = 0.0f;
                constrainColliders(player_getBoulder(playerB), player_getCharacter(playerA), physics,
                                   player_getBoulder(playerB)->collisionMassMultiplier, &relativeSpeed);
                if(gameMode == BATTLE) {
                    updateDamagePlayerToPlayer(playerB, playerA, relativeSpeed, gameState);
                }
            }
        }
        if(gameMode == SURVIVAL) {
            for (j = 0; j < MAX_ENEMIES; ++j) {
                Enemy* enemy = &gameState->enemies[j];
                if (!enemy_exists(enemy)) continue;

                // Check damage player -> enemy.
                {
                    float relativeSpeed = 0.0f;
                    constrainColliders(player_getBoulder(playerA), &enemy->verletBody, physics,
                                       player_getBoulder(playerA)->collisionMassMultiplier, &relativeSpeed);
                    if (relativeSpeed > DAMAGE_THRESHOLD_SPEED && enemy->health > 0) {
                        if(enemy->isBig) {
                            playerA->bigBoulderTimer = BIG_BOULDER_DURATION;
                        }
                        playerA->score++;
                        killEnemy(enemy, gameState);
                        playRandomDrumHard();
                    } else {
                        playRandomDrumSoft();
                    }
                }

                // Check damage enemy -> player.
                {
                    float relativeSpeed = 0.0f;
                    constrainColliders(&enemy->verletBody, player_getCharacter(playerA), physics, 10.0f, &relativeSpeed);
                    if (relativeSpeed > 0 && enemy->health > 0) {
                        killEnemy(enemy, gameState);
                        updateDamageEnemyToPlayer(playerA, gameState);
                    }
                }
            }
        }
    }

    for (i = 0; i < MAX_PLAYERS; ++i) {
        Player* player = &gameState->players[i];
        if(player->health > 0) {
            constrainVerletBody(player_getCharacter(player), &gameState->camera, physics);
        }
    }
}

float updateDeltaTime(Physics* physics) {
    const float c_maxDeltaTime = 1.0f / 30.0f;
    float timeNow = OS_CYCLES_TO_NSEC(osGetTime()) * 1e-9f;
    physics->deltaTime = MIN(c_maxDeltaTime, timeNow - physics->timeAtLastPhysicsUpdate);
    physics->timeAtLastPhysicsUpdate = timeNow;
}

void updatePhysics(GameState* gameState, GameMode gameMode) {
    const Physics* physics = &gameState->physics;
    int i;

    updateDeltaTime(&gameState->physics);

    for(i = 0; i < MAX_PLAYERS; ++i) {
        updatePlayer(&gameState->players[i], &gameState->physics);
    }

    for(i = 0; i < MAX_ENEMIES; ++i) {
        updateEnemy(&gameState->enemies[i], gameState);
    }

    for (i = 0; i < physics->verletConstraintIterations; ++i) {
        constrainPhysics(gameState, gameMode);
    }
}