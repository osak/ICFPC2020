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
import { ReplayList } from "./pages/ReplayList";
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
                  Games (Current Tournament)
                </NavLink>
              </NavItem>
              <NavItem>
                <NavLink tag={Link} to="/games/unrated">
                  Games (Unrated)
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
            render={({ match }) => {
              return <GameList gameType={"Rated"} />;
            }}
          />
          <Route
            exact
            path="/games/rated/:tournamentId"
            render={({ match }) => {
              const tournamentId = match.params.tournamentId;
              return <GameList gameType={"Rated"} tournamentId={tournamentId} />;
            }}
          />
          <Route
            exact
            path="/games/unrated"
            render={({ match }) => {
              return <GameList gameType={"Unrated"} />;
            }}
          />
          <Redirect path="/" to="/" />
        </Switch>
      </Container>
    </HashRouter>
  );
};
