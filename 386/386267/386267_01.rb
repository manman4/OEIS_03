def A386267(n)
  i = 2
  ary = []
  while ary.size < n
    j = Math.sqrt(i).to_i
    if j * j != i or !ary.include?(j)
      ary << i
    end
    i += 1
  end
  ary
end
p A386267(100)
