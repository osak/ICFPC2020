export interface SubmitResponse {
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
