desc 'generate pdf'
task :default do
  sh 'latexmk tex/main.tex'
end

desc 'generate pdf and continuously update'
task :observe do
  sh 'latexmk -pvc -view=none tex/main.tex'
end
