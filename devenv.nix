{ pkgs, lib, config, inputs, ... }:

{
  scripts.gen.exec = "cd $DEVENV_ROOT/ergogen && ergogen .";
  
  enterShell = ''
    echo "Ergogen is ready."
  '';
}
