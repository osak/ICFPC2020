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
import { HistoryList } from "./pages/HistoryList";
import { Visualizer } from "./pages/Visualizer/Visualizer";

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
          <Redirect path="/" to="/" />
        </Switch>
      </Container>
    </HashRouter>
  );
};
