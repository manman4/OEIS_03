def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(n)
  ary = [1]
  (1..n).each{|i|
    if i % 4 == 2
      ary << 0
    else
      s = 0
      (1..i).each{|j|
        s += phi(j) * 2 ** (i / j) if i % j == 0 && j % 2 == 1
      } 
      if i % 4 == 0
        ary << s / i
      else
        ary << s / (2 * i)
      end
    end
  }
  ary
end

n = 3500
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
