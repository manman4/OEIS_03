def A(k, n)
  (1..n).map{|i| (1..i).to_a.join.to_i.to_s(k).to_i}
end

p A(4, 20)