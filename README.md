ICFPC 2020 Team Atelier Manarimo
================================

[Manarimo](https://github.com/osak/ICFPC2018) gets an atelier.

# Team Members
* Kenkou Nakamura (@kenkoooo)
* mkut
* Osamu Koga (@osa_k)
* Shunsuke Ohashi (@pepsin_amylase)
* Yosuke Yano (@y3eadgbe)
* Yu Fujikake (@yuusti)
* Yuki Kawata (@kawatea03)

# Space Battle AI
Source: app/ai/***.h
* Kamikaze (submissions/amylase/kamikaze): Single-ship attacker AI that uses detonate command to kill the defender.
* Bakudan Sanyushi ([ja.wikipedia.org](https://ja.wikipedia.org/wiki/%E7%88%86%E5%BC%BE%E4%B8%89%E5%8B%87%E5%A3%AB)) (submissions/fission_kamikaze): Multi-ship version of Kamikaze. This is used as our team's final submission.
* Meteor Sanyushi (submissions/meteor_bomb): Yet another multi-ship Kamikaze. The difference from Bakudan Sanyushi is the algorithm to operate multi-ships.
* mkut/alisaie: Multi-ship laser attacker. It was not a great idea because the laser component should not be spread.
* mkut/alphinaud: Shoot the laser only if the enemy is on the best angle (0, pi/4, pi/2, ...). Adjusts the position to make the angle better before shooting. This AI sticked to the best angles because we misunderstood the formula of laser damage.
* mkut/titan: Simple AI just to survive in the space. Somethimes the ship falls due to the immature algorithm.
* kawatea/static: Precalculate the stable orbits and use one of them at the runtime. This precalculated orbit is used in other AIs including the last submission (Bakudan Sanyushi).
* kawatea/fission: Generates multiple ships after running on the precalculated orbit.
* yuusti/meteor: Immediately falls on the planet :(
* poyo/goat: The first AI to implement the multi-ship feature.

# Portal
* Atelier Manarimo Portal: https://manarimo.herokuapp.com (we'll shut down it in a few days after contest ended)
* Source: [manarimo/](manarimo/)

Portal website provides web-based communication UI with aliens, scoreboards, replay viewer, and our own Galaxy Pad.

# Galaxy Pad
* Galaxy engine core: [mkut/galaxy/kawatea2.cpp](mkut/galaxy/kawatea2.cpp)
* Interactive UI: [manarimo/web/pages/Visualizer](manarimo/web/pages/Visualizer)

We built a browser-based interactive UI on top of our Galaxy engine written in C++. You can try it in our portal site.

# Contest Feedback
We've really enjoyed the contest! The first hours for figuring out the runes of alien program, exploring the galaxy to find additional puzzles and hidden powers,
reverse engineering the specification of the space battle, and developing the AIs for the battle - all were exciting. Out of all the excellence, we believe following
points are especially noteworthy:

* Infrastructure support provided by the organizer. We've never felt that organizer's system was underpowered. Every game runs pretty quickly, and the information is
  accessible via comprehensive and transparent API.
* Contest structure. Accumulation of scores from short-term ranking seems to be an effective way to disincentivize submarine-ing. Scoreboard was more exciting than ever!
* Throughly prepared game system. 

We are thankful to organizers for all the efforts to make the contest amazing, however, we belive that the lightning round could have been improved. Since we had only
limited information about what is Galaxy function for, it was not quite straightforward to come up with an idea to build a clickable interface for playing around.
It was not necessarily bad as a puzzle, but in our opinion, there were too many misdirections - the incremental uncovering of information in the first hours led us
suspect there would more to come from organizer, lack of explanation in what counts as score made us believe we had to hack the state of Galaxy engine, and prior requests to contribute to Docker system drove us to try sending messages using the submission system.

# Footnote
Recommended: [Atelier Ryza](https://store.steampowered.com/app/1121560/Atelier_Ryza_Ever_Darkness__the_Secret_Hideout/)

Atelier Ryza 2 is coming out this winter!!!!
