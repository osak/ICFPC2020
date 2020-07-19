#include "ai.h"

void run(AI *ai1, AI *ai2) {
    GameInfo info;
    GameState state;

    const StartParams ai1_start_params = ai1->init();
    const StartParams ai2_start_params = ai2->init();

    // STARTコールしてgameResponseが返ってきたつもり
    state.apply_start_params(ai1_start_params);
    state.apply_start_params(ai2_start_params);

    // gameResponseが返ってきたらpost_stateで通知
    ai1->post_start(build_response(info, state));
    ai2->post_start(build_response(info, state));

    int turn = 0;
    while (true) {
        vector<Command*> cmds1 = ai1->think(build_response(info, state));
        vector<Command*> cmds2 = ai2->think(build_response(info, state));
        state.apply(cmds1);
        state.apply(cmds2);

        cout << state.attacker_state.pos << endl;
        cout << state.defender_state.pos << endl;

        if(lose(state.defender_state)) {
            cout << "attacker wins" << endl;
            break;
        } else if (lose(state.attacker_state)) {
            cout << "defender wins" << endl;
            break;
        }

        ++turn;
        if (turn > 255) {
            cout << "Time over" << endl;
            break;
        }
    }
}