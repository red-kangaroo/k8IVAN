COMMAND(ShowKeyLayout) {
  festring Buffer;
  command *cmd;
  //
  felist List(CONST_S("Keyboard Layout"));
  List.AddDescription(CONST_S(""));
  List.AddDescription(CONST_S("Key       Description"));
  for (int c = 0; (cmd = commandsystem::GetCommand(c)); ++c) {
    if (!cmd->IsWizardModeFunction() && (!game::WizardModeIsActive() && cmd->GetName() != "WizardMode")) {
      Buffer.Empty();
      Buffer << cmd->GetKey();
      Buffer.Resize(10);
      List.AddEntry(Buffer+cmd->GetDescription(), LIGHT_GRAY);
    }
  }
  if (game::WizardModeIsActive()) {
    List.AddEntry(CONST_S(""), WHITE);
    List.AddEntry(CONST_S("Wizard mode functions:"), WHITE);
    List.AddEntry(CONST_S(""), WHITE);
    for (int c = 0; (cmd = commandsystem::GetCommand(c)); ++c) {
      if (cmd->IsWizardModeFunction()) {
        Buffer.Empty();
        Buffer << cmd->GetKey();
        Buffer.Resize(10);
        List.AddEntry(Buffer+cmd->GetDescription(), LIGHT_GRAY);
      }
    }
  }
  game::SetStandardListAttributes(List);
  List.Draw();
  return false;
};
