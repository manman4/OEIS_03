def A(n)
  d = 0
  a = (1..n - 1).map{|i| [i]}
  ary = [n - 1]
  while d < n - 2
    d += 1
    b = []
    a.each{|i|
      (1..n - 1).each{|j|
        m = i.clone + [j]
        b << m if (0..d).inject(0){|s, k| s + m[k] * n ** (d - k)} % (d + 1) == 0
      }
    }
    a = b
    ary << a.size
  end
  ary
end

ary = (2..18).map{|i| A(i)}.flatten
(1..ary.size).each{|i|
  print i + 1
  print ' '
  puts ary[i - 1]
}
