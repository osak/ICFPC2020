import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Container, Button, Row, Table } from "reactstrap";
import { GameDto, GamesResponse, PlayerDto } from "../../types";
import { Link } from "react-router-dom";

declare var OFFICIAL_API_BASE: string;
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

function serializeTeamname(player: PlayerDto) {
    if (player.submissionId !== undefined) {
        return `${player.team.teamName} #${player.submissionId}`;
    } else {
        return player.team.teamName;
    }
}

function renderLogLink(player: PlayerDto) {
    if (player.debugLog !== undefined) {
        const url = `${OFFICIAL_API_BASE}/logs?logKey=${encodeURIComponent(player.debugLog)}&apiKey=${OFFICIAL_API_KEY}`
        return <a href={url}>Log</a>
    } else {
        return null;
    }
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
              <th>Finished at</th>
              <th>Visualize</th>
            </tr>
          </thead>
          <tbody>
            {props.games.value.games.map((game: GameDto) => (
              <tr key={game.gameId}>
                <td><span>{serializeTeamname(game.attacker)} </span>{renderLogLink(game.attacker)}</td>
                <td><span>{serializeTeamname(game.defender)} </span>{renderLogLink(game.defender)}</td>
                <td>{game.finishedAt}</td>
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
