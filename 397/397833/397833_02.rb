#!/usr/bin/env ruby
# Brute-force checker for q-ary words containing a fixed word.
#
# Usage:
#   ruby xxx.rb q pattern max_n [--show]
#
# Examples:
#   ruby xxx.rb 2 ababab 12
#   ruby xxx.rb 3 abcabc 9
#   ruby xxx.rb 2 ababab 7 --show

def usage!
  program = File.basename($PROGRAM_NAME)
  puts <<~USAGE
    Usage:
      ruby #{program} q pattern max_n [--show]

    This honestly enumerates all q-ary words of length n for 0 <= n <= max_n
    and counts those containing the given pattern.  It does not use generating
    functions, autocorrelation, automata, or recurrences.

    Arguments:
      q        Alphabet size. For binary words use 2, for ternary words use 3.
      pattern  Word to be contained, e.g., ababab or abcabc.
      max_n    Count all lengths n = 0, 1, ..., max_n.

    Options:
      --show   Print the matching words as well as the counts.

    Examples:
      ruby #{program} 2 ababab 12
      ruby #{program} 3 abcabc 9
      ruby #{program} 2 ababab 7 --show
  USAGE
  exit
end

def alphabet_for(q, pattern)
  used = pattern.chars.uniq
  abort "The pattern uses #{used.length} distinct letters, but q=#{q}." if used.length > q

  candidates = ("a".."z").to_a + ("A".."Z").to_a + ("0".."9").to_a
  alphabet = used.dup
  candidates.each do |c|
    break if alphabet.length == q
    alphabet << c unless alphabet.include?(c)
  end

  abort "Cannot build an alphabet of size #{q} with the built-in symbols." if alphabet.length < q
  alphabet
end

def count_words(alphabet, pattern, n, show)
  count = 0
  matches = []

  build = lambda do |prefix, depth|
    if depth == n
      if prefix.include?(pattern)
        count += 1
        matches << prefix if show
      end
      return
    end

    alphabet.each do |letter|
      build.call(prefix + letter, depth + 1)
    end
  end

  build.call("", 0)
  [count, matches]
end

usage! if ARGV.include?("-h") || ARGV.include?("--help")
show = ARGV.delete("--show") ? true : false

q = Integer(ARGV[0] || usage!)
pattern = ARGV[1] || usage!
max_n = Integer(ARGV[2] || usage!)
usage! if q <= 0 || pattern.empty? || max_n < 0

alphabet = alphabet_for(q, pattern)
terms = []

puts "q = #{q}"
puts "alphabet = {#{alphabet.join(',')}}"
puts "pattern = #{pattern}"
puts "method = brute force enumeration"
puts

0.upto(max_n) do |n|
  count, matches = count_words(alphabet, pattern, n, show)
  terms << count
  puts "a(#{n}) = #{count}"
  puts "  #{matches.join(', ')}" if show && !matches.empty?
end

puts
puts "terms:"
puts terms.join(",")
