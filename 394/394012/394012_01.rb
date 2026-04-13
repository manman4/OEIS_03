def compute_all_d_values(limit)
  # d[j] は現在の m における d_{m, j}
  # 初期条件 d_{1, 0} = 1
  d = [1]
  
  (1..limit).each do |m|
    # m ステップ目の全係数 d_{m, j} を表示
    # puts "--- m = #{m} ---"
    d.each_with_index do |val, j|
      # puts "d(#{m}, #{j}) = #{val}"
      print("#{val}, ")
    end
    # puts "a(#{m}) = #{d[0]}" # これが求める一般項
    # puts ""

    next_d = Array.new(m + 1, 0)
    (0..m).each do |j|
      # 1. α係数: d_{m, j+1} からの寄与
      alpha_term = (j + 1 < d.size) ? 2 * (j + 1) * (2 * j + 1) * d[j + 1] : 0
      
      # 2. β係数: d_{m, j} からの寄与
      beta_term = (j < d.size) ? 4 * (4 * m * j + 2 * j**2 + m + j) * d[j] : 0
      
      # 3. γ係数: d_{m, j-1} からの寄与
      gamma_term = (j > 0) ? 2 * (4 * m + 2 * j + 1) * (2 * m + j - 1) * d[j - 1] : 0
      
      next_d[j] = alpha_term + beta_term + gamma_term
    end
    d = next_d
  end
end

compute_all_d_values(7)