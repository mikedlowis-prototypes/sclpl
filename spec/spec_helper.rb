require 'open3'

def cli(options, input = "")
  out, err, status = Open3.capture3(
      *(['./build/bin/sclpl-test'] + options + [{:stdin_data => input}]))
  raise err unless err == ""
  raise "Command returned non-zero status" unless status.success?
  out
end

def lexer(input)
  cli(['--tokens'], input).scan(/^(T_[A-Z]+(:("[^"]*"|[^\n]+))?)/m).map {|m| m[0] }
end

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

def ast(input)
  out = cli(['--ast'], input)
  # Prep the parens for reading
  out.gsub!(/([()])|tree/,' \1 ')
  # Replace string literals so we can tokenize on spaces
  strings = []
  out.gsub!(/"[^\"]*"/) do |m|
      strings << m
      '$_$_STRING_$_$'
  end
  # Put the strings back in after splitting
  tokens = out.split.map do |tok|
    if tok.end_with? '$_$_STRING_$_$'
      tok.gsub('$_$_STRING_$_$', strings.shift)
    else
      tok
    end
  end
  # Build the tree structure and return the result
  off, expr = re_structure(tokens)
  expr
end

def ccode(input)
  cli(['--csource'], input)
end
