def A006950(n)
  a = (1..n - 1).map{|i| [i]}
  ary = [[n]]
  while a.size != 0
    b = []
    a.each{|i|
      x = i[-1].to_i
      s = n - i.inject(:+)
      t = x % 2
      (x + t..s - 1).each{|j| b << i.clone + [j]}
      ary << i.clone + [s] if s >= x + t
    }
    a = b
  end
  b = Array.new(n + 1, 0)
  ary.each{|i|
    b[i[-1]] += 1
  }
  b
end

n = 50
ary = []
(0..n).each{|i|
  a = A006950(i)
  ary << a
}
ary.flatten!
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
