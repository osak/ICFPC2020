CREATE TABLE aliens (
  id INT GENERATED ALWAYS AS IDENTITY,
  raw_request text NOT NULL,
  request text NOT NULL,
  raw_response text DEFAULT NULL,
  response text DEFAULT NULL,
  send_at TIMESTAMPTZ NOT NULL
);
