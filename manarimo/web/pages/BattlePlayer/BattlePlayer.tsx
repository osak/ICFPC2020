import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Alert, Spinner } from "reactstrap";
import { RelayData } from "../../types";
import { BattleViewer } from "./BattleViewer";

declare var API_BASE: string;
const REPLAY_API = `${API_BASE}/replay`;

interface OuterProps {
  replayId: string;
}

interface InnerProps extends OuterProps {
  replay_data: PromiseState<RelayData>;
}

const InnerBattlePlayer = (props: InnerProps) => {
  if (props.replay_data.pending) {
    return <Spinner />;
  }
  if (props.replay_data.rejected) {
    return <Alert color="danger">Server Error</Alert>;
  }
  return (
    <BattleViewer
      replayData={props.replay_data.value}
      replayId={props.replayId}
    />
  );
};

export const BattlePlayer = connect<OuterProps, InnerProps>((props) => ({
  replay_data: {
    url: REPLAY_API,
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ playerKey: props.replayId }),
  },
}))(InnerBattlePlayer);
