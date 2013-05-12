@echo off
md   ..\..\..\devel29                                           > nul 2>&1
md   ..\..\..\output29                                          > nul 2>&1
md   ..\..\..\devel29\share                                     > nul 2>&1
md   ..\..\..\output29\share                                    > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks                          > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks                         > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks\images                   > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks\images                  > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks\images\fortranproject    > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks\images\fortranproject   > nul 2>&1

copy images\fortranproject\*.*       ..\..\..\devel29\share\CodeBlocks\images\fortranproject\     > nul 2>&1
copy images\fortranproject\*.*       ..\..\..\output29\share\CodeBlocks\images\fortranproject\    > nul 2>&1

exit 0
