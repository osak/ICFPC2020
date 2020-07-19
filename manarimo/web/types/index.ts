export interface SubmitResponse {
  raw_request: string;
  raw_response: string;
  request: string;
  response: string;
  timestamp: string;
}

export interface HistoryItem extends SubmitResponse {
  id: number;
}

export interface SendHistory {
  items: HistoryItem[];
}

export interface TurnReplay {
  data: {
    command: unknown;
    state: {
      max_velocity: number;
      velocity: { x: number; y: number };
      heat: number;
      location: { x: number; y: number };
      is_attacker: boolean;
      max_heat: number;
      is_defender: boolean;
      parameters: {
        attack: number;
        fuel: number;
        cool_speed: number;
        health: number;
      };
    };
  }[];
  turn_id: number;
}

export interface RelayData {
  data: {
    a: number;
    b: number;
    c: number;
    details: {
      planet_size: number;
      space_size: number;
      turns: TurnReplay[];
    };
  };
}

export interface TournamentDto {
  tournamentId: number;
  tournamentType: TournamentType;
}

export type TournamentType = "Lightning" | "Regular" | "Finals";
export type GameType = "Rated" | "Unrated";

export interface TeamDto {
  teamName: string;
}

export interface PlayerDto {
  submissionId?: number;
  team: TeamDto;
  playerKey: string;
  debugLog?: string;
}

export interface GameDto {
  gameId: string;
  attacker: PlayerDto;
  defender: PlayerDto;
  finishedAt: string;
}

export interface GamesResponse {
  hasMore: boolean;
  next: string;
  games: Array<GameDto>;
}
