import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Alert, Spinner } from "reactstrap";
import { SavedGameList, SubmissionResponse } from "../../types";
import { GameSearchTable } from "./GameSearchTable";

declare var API_BASE: string;
const GAME_LIST_API = `${API_BASE}/games`;
const SUBMISSIONS_API = `${API_BASE}/submissions`;

interface Props {
  list: PromiseState<SavedGameList>;
  submissions: PromiseState<SubmissionResponse>;
}

const InnerSavedBattleList = (props: Props) => {
  if (props.list.pending || props.submissions.pending) {
    return <Spinner />;
  }
  if (props.list.rejected || props.submissions.rejected) {
    return <Alert color="danger">Server Error</Alert>;
  }
  return (
    <GameSearchTable
      list={props.list.value}
      submissions={props.submissions.value}
    />
  );
};
export const SavedBattleList = connect<{}, Props>(() => ({
  list: GAME_LIST_API,
  submissions: SUBMISSIONS_API,
}))(InnerSavedBattleList);
