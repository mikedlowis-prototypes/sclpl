require 'open3'

def lexer(input)
  out, err, status = Open3.capture3('./build/bin/sclpl-test', '--tokens', :stdin_data => input)
  raise "Lexer command returned non-zero status" unless status.success?
  raise "Lexer produced error messages" unless err == ""
  out.gsub(/<tok (T_[A-Z]+)>/,'\1').split
end

describe "lexer" do
  it "should recognize punctuation" do
    expect(lexer('[](){};\'",')).to eq(
      ["T_LBRACK", "T_RBRACK", "T_LPAR", "T_RPAR", "T_VAR", "T_VAR", "T_END"])
  end
end