import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Container, Button, Row, Table } from "reactstrap";
import { GameDto, GamesResponse } from "../../types";
import { Link } from "react-router-dom";

declare var OFFICIAL_API_KEY: string;

interface GamesFetchBase {
  apiUrl: string;
  tournamentId?: number;
}

interface GamesState {
  games: Array<GameDto>;
  lastBefore: string;
  hasNext: boolean;
}

interface Props {
  gamesFetchBase: GamesFetchBase;
}

interface InnerProps extends Props {
  updateGames: (currentState: GamesState) => any;
  games: PromiseState<GamesState>;
}

const InnerGameTable = (props: InnerProps) => {
  if (props.games.pending) {
    return <span>Loading...</span>;
  }

  if (props.games.rejected) {
    return <span>Failed to load</span>;
  }

  return (
    <Container>
      <Row>
        <Table>
          <thead>
            <tr>
              <th>Attacker</th>
              <th>Defender</th>
              <th>Visualize</th>
            </tr>
          </thead>
          <tbody>
            {props.games.value.games.map((game: GameDto) => (
              <tr key={game.gameId}>
                <td>{game.attacker.team.teamName}</td>
                <td>{game.defender.team.teamName}</td>
                <td>
                  <Link to={`/replay/${game.attacker.playerKey}`}>Go</Link>
                </td>
              </tr>
            ))}
          </tbody>
        </Table>
      </Row>
      <Row>
        <Button
          disabled={!props.games.value.hasNext}
          onClick={() => {
            // @ts-ignore
            props.updateGames(props.games.value);
          }}
        >
          Load more...
        </Button>
      </Row>
    </Container>
  );
};

// @ts-ignore
export const GameTable = connect<Props, InnerProps>((props: Props) => {
  let url = `${props.gamesFetchBase.apiUrl}?apiKey=${OFFICIAL_API_KEY}`;
  if (props.gamesFetchBase.tournamentId !== undefined) {
    url += `&tournamentId=${props.gamesFetchBase.tournamentId}`;
  }
  return {
    games: {
      url,
      then: (gamesResponse: GamesResponse) => ({
        value: {
          games: gamesResponse.games,
          lastBefore: gamesResponse.next,
          hasNext: gamesResponse.hasMore,
        },
      }),
    },
    updateGames: (currentState: GamesState) => {
      const fullUrl =
        url + `&before=${encodeURIComponent(currentState.lastBefore)}`;
      return {
        games: {
          url: fullUrl,
          then: (gamesResponse: GamesResponse) => ({
            value: {
              games: currentState.games.concat(gamesResponse.games),
              lastBefore: gamesResponse.next,
              hasNext: gamesResponse.hasMore,
            },
          }),
        },
      };
    },
  };
})(InnerGameTable);
