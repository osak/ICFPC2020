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

export const parseCommand = (commandObject: unknown): CommandType | null => {
  if (
    commandObject === null ||
    commandObject === undefined ||
    !Array.isArray(commandObject)
  ) {
    return null;
  }

  const commandArray = commandObject[0];
  switch (commandArray[0]) {
    case JET:
      return parseJetCommand(commandArray);
    case SUICIDE:
      return parseSuicide(commandArray);
    case ATTACK:
      return parseAttack(commandArray);
    default:
      return null;
  }
};
