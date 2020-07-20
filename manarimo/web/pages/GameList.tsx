import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Container, Row } from "reactstrap";
import { TournamentDto, GameType } from "../types";
import { GameTable } from "./GameList/GameTable";

declare var OFFICIAL_API_BASE: string;
const CURRENT_TOURNAMENT_API = `${OFFICIAL_API_BASE}/tournaments/current`;

interface Props {
  gameType: GameType;
  tournamentId?: number;
}

interface InnerProps extends Props {
  currentTournamentResponse: PromiseState<TournamentDto>;
}

const InnerGameList = (props: InnerProps) => {
  const currentTournamentId = props.currentTournamentResponse.fulfilled
    ? props.currentTournamentResponse.value.tournamentId
    : 0;

  let gamesFetchBase;
  if (props.gameType === "Rated") {
    gamesFetchBase = {
      apiUrl: `${OFFICIAL_API_BASE}/games`,
      tournamentId:
        props.tournamentId !== undefined
          ? props.tournamentId
          : currentTournamentId,
    };
  } else {
    gamesFetchBase = {
      apiUrl: `${OFFICIAL_API_BASE}/games/non-rating`,
    };
  }

  return <GameTable gamesFetchBase={gamesFetchBase} />;
};

export const GameList = connect<Props, InnerProps>(() => ({
  currentTournamentResponse: CURRENT_TOURNAMENT_API,
}))(InnerGameList);
