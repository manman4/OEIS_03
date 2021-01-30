require 'prime'

def d(n)
  s = 0
  (1..n).each{|i|
    s += 1 if n % i == 0
  }
  s
end

n = 1000
# Œë·‚ª‘½‚¢
p ary = (1..n).map{|i| (d(i) * i ** 5.5).to_i}
=begin
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
=end

def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    s, t, u = 0, 1, 0
    (1..n).each{|j|
      t += 9 * j
      u += j
      break if i <= u
      s += (-1) ** (j % 2 + 1) * (2 * j + 1) * (i - t) * ary[-u]
    }
    ary << s / (i - 1)
  }
  ary
end

=begin
n = 30000
ary = A(n)
(1..n).each{|i|
  if !i.prime? && (ary[i].abs + 0.0) / (d(i) * i ** 5.5).to_i > 0.7
  print i
  print ' | '
  print ary[i].abs
  print ' | '
  print (d(i) * Math.sqrt(i ** 11)).to_i
  print ' | '
  puts (ary[i].abs + 0.0) / (d(i) * Math.sqrt(i ** 11)).to_i
end
}
=end
