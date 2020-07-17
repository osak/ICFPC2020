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
