#!/bin/bash
# usage: commit.sh "<message>" 
cd ~/dev/xenogears-decomp-public-harvest
git add -A src NOTICE.md LICENSE docs gears.toml config 2>/dev/null
git -c user.name="Blizz127" -c user.email="16393800+Blizz127@users.noreply.github.com" commit -q -m "$1" && git log --oneline -1
git push origin decomp/public-harvest-20261109 2>&1 | tail -1
