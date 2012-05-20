COMMAND(IssueCommand) {
  if (!Char->CheckTalk()) return false;
  return game::CommandQuestion();
};
