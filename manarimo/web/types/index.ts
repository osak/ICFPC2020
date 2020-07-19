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
    command: any;
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
