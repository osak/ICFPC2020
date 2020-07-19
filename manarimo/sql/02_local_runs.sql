CREATE TABLE local_runs (
  id INT GENERATED ALWAYS AS IDENTITY,
  attacker_name text NOT NULL,
  attacker_key text NOT NULL,
  defender_name text NOT NULL,
  defender_key text NOT NULL,
  send_at TIMESTAMPTZ NOT NULL
);
