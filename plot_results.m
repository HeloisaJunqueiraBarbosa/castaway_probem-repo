
file = fopen('...castaway_probem-repo-master/world/data.csv');
tline = fgetl(file);
figure();

while ischar(tline)
    C2 = textscan(tline,'%f32 %f32 %f32 %f32 %f32','Delimiter',';');
    plot(C2{2}, C2{3},'bo', C2{4}, C2{5},'ro');
    axis([-1.2 1.2 -1.2 1.2]);
    tline = fgetl(files);
    pause(0.00000001);
    hold on
    grid on;
end
fclose(file);
hold off