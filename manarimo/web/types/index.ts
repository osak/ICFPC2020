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
  finishedAt?: string;
  winner?: "Defender" | "Attacker" | "Nobody";
}

export interface GamesResponse {
  hasMore: boolean;
  next: string;
  games: Array<GameDto>;
}

export interface LocalRunEntry {
  id: number;
  attacker_key: string;
  attacker_name: string;
  defender_key: string;
  defender_name: string;
  timestamp: string;
}

export interface LocalRunResponse {
  items: LocalRunEntry[];
}

export interface SavedGame {
  id: number;
  game_id: string;
  tournament_id: string;
  played_at: string;
  ticks: number;
  winner: "Attacker" | "Defender" | "Nobody";
  winner_team_name: string;

  attacker_submission_id: number;
  attacker_team_id: string;
  attacker_team_name: string;
  attacker_player_key: string;
  attacker_debug_log: string | null;

  defender_submission_id: number;
  defender_team_id: string;
  defender_team_name: string;
  defender_player_key: string;
  defender_debug_log: string | null;
}

export interface SavedGameList {
  items: SavedGame[];
}

export interface Submission {
  alias: string | null;
  branch_name: string;
  commit_message: string;
  create_at: string | null;
  submission_id: number;
}

export interface SubmissionResponse {
  items: Submission[];
}
