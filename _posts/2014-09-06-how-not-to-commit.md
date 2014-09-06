---
layout: post
title: How not to commit to GitHub
---

*Sometimes you forget that just because it feels like you know what you're doing, you might not.* This is why I now have a commit that looks like this "WIP on master: 70a43e6 Committing changes that lead up to Issue #3"

-----

While making a few changes to the core profiling product I decided it was time to stash my changes instead of committing them. I read some documentation and felt like I understood the process pretty well. Satisfied that I hadn't left my code in a bad state I ran the command:

```
git stash
```

A not so useful message came up and said that it was unable to log to the repo logs and then mashed in the command line was **WIP on master: 70a43e6 Committing changes that lead up to Issue #3**. I felt like this was a success so I closed my IDE and went home. Later in the evening I wanted to pull my stash down to my home PC.

Well this is where I *completely* lost touch with what I thought stashing changes was. I won't repeat all that I learned, but if you're a person who is used to TFS and shelvesets, this is nothing like them. I was able to log into my remote machine only to find that I was unable to list the stash. So I tried the next logical thing and tried to apply the stash. This also did not work. ... panic.

Now, faced with losing a day of solid work and not knowing what to do I went back to Google. Every now and then when you start googling you get this sinking feeling that you don't know what you're asking for, or you're the only person IN THE ENTIRE WORLD to have this problem. Luckily only one of these paradigms are true.

Once I calmed down and started reading some stackoverflow and some other blog posts about the issue I was sure that I was alone. But, a ray of light shown through once I found Thomas Ferris Nicolaisen's blog and his [blooper](http://blog.tfnico.com/2012/09/git-stash-blooper-could-not-restore.html). Once I found the proper command to run my life was a whole lot better.

```
$ git log --graph --all --decorate --oneline
* af75065 (HEAD, origin/gh-pages, gh-pages) Maybe this time...
* bdfbf5c Updated config to get the links correct.
* 0c03dbe Adding poole jekyll blog.
| *   e27feb3 (refs/stash) WIP on gh-pages: 53f9f8b Create gh-pages branch via G
| |\
|/ /
| * 97f0846 index on gh-pages: 53f9f8b Create gh-pages branch via GitHub
|/
* 53f9f8b Create gh-pages branch via GitHub
```

Once I found the stash point I wanted to revert back to I ran the following:

```
git checkout e27feb3
```

BAM! All of my changes were back.  I instantly saved a copy of that directory because now, more than ever, I felt like I was going to run some errant command and delete my entire repository and all of my copies in one fell swoop. Unfortunately I was in a detached state so in a haste I pushed all of my changes back to the MASTER and was done with it. I found the answer on [this](http://stackoverflow.com/a/10738921) stackoverflow answer.

```
git push origin HEAD:master
```

It was certainly ugly and I didn't want to lose my changes after hitting one of the best strides I had in a while. If you somehow stumble on this blog please pay more respect to the folks whos page's I ripped the answers from to cobble together this awful solution. :)