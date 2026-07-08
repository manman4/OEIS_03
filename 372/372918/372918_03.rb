def a_number(n, s = 3, t = 2)
  return 0 if n <= 0

  nt = n**t
  total = 0
  1.upto(n){|k|
    total += (k**s).gcd(nt)
  }
  total
end

def sequence(limit, s = 3, t = 2)
  (0..limit).map{|n| a_number(n, s, t)}
end

if __FILE__ == $PROGRAM_NAME
  limit = (ARGV[0] || 20).to_i
  s = (ARGV[1] || 3).to_i
  t = (ARGV[2] || 2).to_i

  ary = sequence(limit, s, t)
  (1..limit).each{|n|
    j = ary[n]
    break if j.to_s.size > 1000
    print n
    print ' '
    puts j
  }
end
