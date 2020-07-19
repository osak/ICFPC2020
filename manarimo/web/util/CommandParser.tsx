const JET = 0;
const SUICIDE = 1;
const ATTACK = 2;

const parseJetCommand = (command: unknown[]) => {
  if (!Array.isArray(command[1])) {
    return null;
  }
  const jetVector = command[1];
  if (typeof jetVector[0] !== "number" || typeof jetVector[1] !== "number") {
    return null;
  }

  return {
    command: "JET" as const,
    x: jetVector[0],
    y: jetVector[1],
  };
};

const parseAttack = (command: unknown[]) => {
  if (!Array.isArray(command[1])) {
    return null;
  }

  const attackLocation = command[1];
  if (
    typeof attackLocation[0] !== "number" ||
    typeof attackLocation[1] !== "number"
  ) {
    return null;
  }
  return {
    command: "ATTACK" as const,
    x: attackLocation[0],
    y: attackLocation[1],
  };
};

const parseSuicide = (command: unknown[]) => {
  return { command: "SUICIDE" as const };
};

type CommandType =
  | ReturnType<typeof parseAttack>
  | ReturnType<typeof parseJetCommand>
  | ReturnType<typeof parseSuicide>;

const parseSingleCommand = (command: unknown): CommandType | null => {
  if (!Array.isArray(command)) {
    return null;
  }
  switch (command[0]) {
    case JET:
      return parseJetCommand(command);
    case SUICIDE:
      return parseSuicide(command);
    case ATTACK:
      return parseAttack(command);
    default:
      return null;
  }
};

export const parseCommand = (commands: unknown): CommandType[] => {
  if (!Array.isArray(commands)) {
    return [];
  }

  return commands.map((command) => parseSingleCommand(command));
};
