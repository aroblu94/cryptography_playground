# Cryptography playground
Personal college Cryptography exercises written in `C`.    
    
## Test & Run
Compile `main.c` in each subfolder with `gcc main.c -o main.o` and execute with `./main.o`.
    
## Contents
- `sdes_three/`: Simplified DES (DES with rounds 2, 3 and 4)
- `sdes_four/`: Simplified DES (DES with 1 to 4 rounds)
- `atk_sdes_three/`: Differential cryptoanalysis of 3-rounds S-DES
    

## Notes
I'm currently using `git-flow` ([http://danielkummer.github.io/git-flow-cheatsheet/](http://danielkummer.github.io/git-flow-cheatsheet/)) to track my work.   
Basically I'll develop every lesson as a separated branch (`l<#lesson>`) from `develop` (I'll try to remember to track them). Once completed a lesson I'll merge it into `develop`, releasing a.. ehm...`release` (will try to track them too) with a `tag` and I'll push it to the `master` passing through `release/<#release>` branch.    
This is how it worked till now (well, it's my first release, so funny) but things *might* change. After all it is a **playground**, isn't it?  
    
In order to use `GMP` (Gnu MP) library you have to install it first from [here](https://gmplib.org), hence compile with `-lgmp` param.   