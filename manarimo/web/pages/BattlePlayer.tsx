import { connect, PromiseState } from "react-refetch";
import React from "react";
import { RelayData } from "../types";

declare var API_BASE: string;
const REPLAY_API = `${API_BASE}/replay`;

interface OuterProps {
  replayId: string;
}

interface InnerProps extends OuterProps {
  replay_data: PromiseState<RelayData>;
}

const InnerBattlePlayer = (props: InnerProps) => {
  if (props.replay_data.fulfilled) {
    console.log(props.replay_data.value);
  }
  return <>battle {props.replayId}</>;
};

export const BattlePlayer = connect<OuterProps, InnerProps>((props) => ({
  replay_data: {
    url: REPLAY_API,
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ playerKey: props.replayId }),
  },
}))(InnerBattlePlayer);
