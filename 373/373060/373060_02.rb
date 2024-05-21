def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    s = 0
    (1..i).each{|j|
      (1..i).each{|k|
        (1..i).each{|l|
          m = i.gcd(j)
          s += m / (m.gcd(k).gcd(l))
        }
      }
    }
    ary << s
  }
  ary
end

n = 200
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}