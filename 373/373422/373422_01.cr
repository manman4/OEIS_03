def diff(ary, n)
  a = ary.clone
  (n - 1).times{|i|
    (0..n - 2 - i).each{|j|
      a[j] = a[j + 1] - a[j]
    }
    a.pop
  }
  a[0] == 0
end

def a(n)
  ary = [0] * (n + 1)
  (1..n).to_a.each_permutation{|i|
    ary[i[0]] += 1 if diff(i, n)
  }
  ary[1..-1]
end

n = 11
ary = (1..n).map{|i| a(i)}.flatten
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
