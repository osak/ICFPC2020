import { Redirect } from "@reach/router";
import React, { useState } from "react";
import {
  Collapse,
  Navbar,
  NavbarToggler,
  NavbarBrand,
  Nav,
  NavItem,
  NavLink,
  Container,
} from "reactstrap";
import { HashRouter, Switch, Route, Link } from "react-router-dom";
import { AlienSend } from "./pages/AlienSend";
import { BattlePlayer } from "./pages/BattlePlayer/BattlePlayer";
import { HistoryList } from "./pages/HistoryList";
import { LocalGameList } from "./pages/LocalGameList";
import { ReplayList } from "./pages/ReplayList";
import { SavedBattleList } from "./pages/SavedBattleList/SavedBattleList";
import { Visualizer } from "./pages/Visualizer/Visualizer";
import { GameList } from "./pages/GameList";

export const App = () => {
  const [isOpen, setIsOpen] = useState(false);

  const toggle = () => setIsOpen(!isOpen);
  return (
    <HashRouter>
      <div>
        <Navbar color="light" light expand="md">
          <NavbarBrand tag={Link} to="/">
            Atelier Manarimo
          </NavbarBrand>
          <NavbarToggler onClick={toggle} />
          <Collapse isOpen={isOpen} navbar>
            <Nav className="mr-auto" navbar>
              <NavItem>
                <NavLink tag={Link} to="/submit">
                  Submit
                </NavLink>
              </NavItem>
              <NavItem>
                <NavLink tag={Link} to="/vis">
                  Visualizer
                </NavLink>
              </NavItem>
              <NavItem>
                <NavLink tag={Link} to="/replay">
                  Replay
                </NavLink>
              </NavItem>
              <NavItem>
                <NavLink tag={Link} to="/games/current">
                  Current Tournament
                </NavLink>
              </NavItem>
              <NavItem>
                <NavLink tag={Link} to="/games/unrated">
                  Unrated
                </NavLink>
              </NavItem>
              <NavItem>
                <NavLink tag={Link} to="/games/local">
                  Local
                </NavLink>
              </NavItem>
            </Nav>
            <Nav className="ml-auto" navbar>
              <NavLink
                href="https://icfpcontest2020.github.io/#/scoreboard"
                target="_blank"
              >
                Scoreboard
              </NavLink>
            </Nav>
          </Collapse>
        </Navbar>
      </div>
      <Container fluid className="my-5">
        <Switch>
          <Route exact path="/" component={HistoryList} />
          <Route exact path="/submit" component={AlienSend} />
          <Route exact path="/vis" component={Visualizer} />
          <Route exact path="/replay" component={ReplayList} />
          <Route exact path="/games" component={SavedBattleList} />
          <Route
            exact
            path="/replay/:replayId"
            render={({ match }) => {
              const replayId = match.params.replayId ?? "";
              return <BattlePlayer replayId={replayId} />;
            }}
          />
          <Route
            exact
            path="/games/current"
            render={() => {
              return <GameList gameType={"Rated"} />;
            }}
          />
          <Route
            exact
            path="/games/rated/:tournamentId"
            render={({ match }) => {
              const tournamentId = match.params.tournamentId;
              return (
                <GameList gameType={"Rated"} tournamentId={tournamentId} />
              );
            }}
          />
          <Route
            exact
            path="/games/unrated"
            render={() => {
              return <GameList gameType={"Unrated"} />;
            }}
          />
          <Route exact path="/games/local" component={LocalGameList} />
          <Redirect path="/" to="/" />
        </Switch>
      </Container>
    </HashRouter>
  );
};
