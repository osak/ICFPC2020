import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Alert, Spinner } from "reactstrap";
import { SavedGameList } from "../../types";
import { GameSearchTable } from "./GameSearchTable";

declare var API_BASE: string;
const GAME_LIST_API = `${API_BASE}/games`;

interface Props {
  list: PromiseState<SavedGameList>;
}

const InnerSavedBattleList = (props: Props) => {
  if (props.list.pending) {
    return <Spinner />;
  }
  if (props.list.rejected) {
    return <Alert color="danger">Server Error</Alert>;
  }
  return <GameSearchTable list={props.list.value} />;
};
export const SavedBattleList = connect<{}, Props>(() => ({
  list: GAME_LIST_API,
}))(InnerSavedBattleList);
