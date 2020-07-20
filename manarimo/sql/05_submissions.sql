CREATE TABLE submissions (
  submission_id INT PRIMARY KEY,
  branch_name VARCHAR(255) NOT NULL,
  commit_message TEXT DEFAULT NULL,
  alias VARCHAR(255) DEFAULT NULL,
  created_at TIMESTAMPTZ DEFAULT NULL
);

CREATE INDEX ON submissions (alias);
CREATE INDEX ON submissions (created_at);