def A(k, n)
  (1..n).map{|i| (1..i).to_a.reverse.join.to_i.to_s(k).to_i}
end

p A(9, 16)