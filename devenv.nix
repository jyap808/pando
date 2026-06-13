{ pkgs, lib, config, inputs, ... }:

{
  scripts.gen.exec = "cd $DEVENV_ROOT/ergogen && ergogen .";
  scripts.gensvg.exec = "cd $DEVENV_ROOT/ergogen && ergogen --svg .";
  
  enterShell = ''
    echo "Ergogen is ready."
  '';
}
