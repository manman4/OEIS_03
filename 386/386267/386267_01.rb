def A386267(n)
  i = 2
  ary = []
  while ary.size < n
    j = Math.sqrt(i).to_i
    if j * j != i || !ary.include?(j)
      ary << i
    end
    i += 1
  end
  ary
end

n = 10000
ary = A386267(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
