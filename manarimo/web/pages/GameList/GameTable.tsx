import React, { useState } from "react";
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
  games: GameDto[];
  lastBefore: string;
  hasNext: boolean;
}

interface Props {
  gamesFetchBase: GamesFetchBase;
}

interface InnerProps extends Props {
  updateGames: (currentState: GamesState) => any;
  games: PromiseState<GamesResponse>;
}

function serializeTeamName(player: PlayerDto) {
  const teamName = player.team.teamName.startsWith("Atelier Manarimo") ? (
    <b>{player.team.teamName}</b>
  ) : (
    player.team.teamName
  );

  if (player.submissionId !== undefined) {
    return (
      <>
        {teamName}
        {` #${player.submissionId}`}
      </>
    );
  } else {
    return teamName;
  }
}

function renderLogLink(player: PlayerDto) {
  if (player.debugLog !== undefined) {
    const url = `${OFFICIAL_API_BASE}/logs?logKey=${encodeURIComponent(
      player.debugLog
    )}&apiKey=${OFFICIAL_API_KEY}`;
    return (
      <a href={url} target="_blank">
        Log
      </a>
    );
  } else {
    return null;
  }
}

const InnerGameTable = (props: InnerProps) => {
  const [showCount, setShowCount] = useState(300);
  const games = props.games.value;

  return (
    <Container fluid>
      <Row>
        <Table>
          <thead>
            <tr>
              <th />
              <th>Attacker</th>
              <th />
              <th>Defender</th>
              <th>Finished at</th>
              <th>Visualize</th>
            </tr>
          </thead>
          <tbody>
            {games &&
              games.games.slice(0, showCount).map((game: GameDto) => {
                return (
                  <tr key={game.gameId}>
                    <td>{game.winner === "Attacker" && "🏆"}</td>
                    <td>
                      <span>{serializeTeamName(game.attacker)} </span>
                      {renderLogLink(game.attacker)}
                    </td>
                    <td>{game.winner === "Defender" && "🏆"}</td>
                    <td>
                      <span>{serializeTeamName(game.defender)} </span>
                      {renderLogLink(game.defender)}
                    </td>
                    <td>
                      {game.finishedAt &&
                        new Date(game.finishedAt).toLocaleString()}
                    </td>
                    <td>
                      {game.finishedAt && (
                        <>
                          <a
                            href={`https://icfpcontest2020.github.io/#/visualize?game=${game.gameId}`}
                          >
                            Official
                          </a>
                          {" | "}
                          <Link to={`/replay/${game.attacker.playerKey}`}>
                            Manarimo
                          </Link>
                        </>
                      )}
                    </td>
                  </tr>
                );
              })}
          </tbody>
        </Table>
      </Row>
      <Row>
        <Button
          disabled={!games || games.games.length <= showCount}
          onClick={() => {
            if (games) {
              setShowCount(showCount + 300);
            }
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
  let url = `${props.gamesFetchBase.apiUrl}?take=300&apiKey=${OFFICIAL_API_KEY}`;
  if (props.gamesFetchBase.tournamentId !== undefined) {
    url += `&tournamentId=${props.gamesFetchBase.tournamentId}`;
  }
  return {
    games: url,
  };
})(InnerGameTable);
