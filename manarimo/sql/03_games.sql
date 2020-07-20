CREATE TABLE games (
  id INT GENERATED ALWAYS AS IDENTITY,
  game_id VARCHAR(255) NOT NULL UNIQUE,
  tournament_id INT NOT NULL,
  played_at TIMESTAMPTZ NOT NULL,
  ticks INT NOT NULL,
  winner VARCHAR(255) NOT NULL,
  winner_team_name VARCHAR(255) NOT NULL,

  attacker_submission_id INT NOT NULL,
  attacker_team_id VARCHAR(255) NOT NULL,
  attacker_team_name VARCHAR(255) NOT NULL,
  attacker_player_key VARCHAR(32) NOT NULL,
  attacker_debug_log VARCHAR(255) DEFAULT NULL,

  defender_submission_id INT NOT NULL,
  defender_team_id VARCHAR(255) NOT NULL,
  defender_team_name VARCHAR(255) NOT NULL,
  defender_player_key VARCHAR(32) NOT NULL,
  defender_debug_log VARCHAR(255) DEFAULT NULL
);

CREATE INDEX ON games (winner_team_name, played_at);
CREATE INDEX ON games (tournament_id, played_at);
CREATE INDEX ON games (game_id);