require 'open3'

def re_structure( token_array, offset = 0 )
  struct = []
  while( offset < token_array.length )
    if(token_array[offset] == "(")
      # Multiple assignment from the array that re_structure() returns
      offset, tmp_array = re_structure(token_array, offset + 1)
      struct << tmp_array
    elsif(token_array[offset] == ")")
      break
    else
      struct << token_array[offset]
    end
    offset += 1
  end
  return [offset, struct]
end

def parser(input)
  out, err, status = Open3.capture3('./build/bin/sclpl-test', '--ast', :stdin_data => input)
  raise "Parser command returned non-zero status" unless status.success?
  raise "Parser produced error messages" unless err == ""
  out.gsub!(/<tok (T_[A-Z]+)>/,'\1')
  out.gsub!(/([()])|tree/,' \1 ')
  off, expr = re_structure(out.split)
  expr
end

describe "parser" do
  it "should parse a definition" do
    expect(parser('def foo 123;')).to eq([ ['T_VAR', 'T_VAR', 'T_INT'] ])
  end
end
