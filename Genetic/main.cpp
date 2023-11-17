#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

struct Teacher {
    std::string name;
    std::vector<bool> subjectKnowledge;
    int maxHours;
};

struct Group {
    std::string name;
    std::vector<bool> subjects;
};

struct Timeslot {
    int subjectIdx;
    int teacherIdx;
    int groupIdx;
};

std::vector<std::string> subjects = {
    "Discrete Math", "Physics", "Programming", "Chemistry", "Philosophy", "Calculus"
};

std::vector<Teacher> teachers = {
    {"Tom", {true, true, false, false, false, false}, 6},
    {"Bob", {false, false, true, true, false, false}, 5},
    {"Mary", {false, false, false, false, true, true}, 7}
};

std::vector<Group> groups = {
    {"A", {false, false, false, false, true, false}},
    {"B", {false, true, false, true, false, true}},
    {"C", {true, false, true, false, false, false}}
};

size_t numTeachers = teachers.size();
size_t numGroups = groups.size();
size_t numSubjects = subjects.size();
size_t numTimeslots = 15;
size_t populationSize = 10;

size_t numGenerations = 500;
size_t printStep = numGenerations / 15;

class Schedule {
public:
    Schedule() {
        timetable.resize(numTimeslots);
        generateRandomSchedule();
    }

    void generateRandomSchedule() {
        for (size_t timeslotIdx = 0; timeslotIdx < timetable.size(); ++timeslotIdx) {
            timetable[timeslotIdx].subjectIdx = rand() % numSubjects;
            timetable[timeslotIdx].teacherIdx = rand() % numTeachers;
            timetable[timeslotIdx].groupIdx = rand() % numGroups;
        }
    }

    int fitness() const {
        int totalPenalty = 0;

        for (size_t teacherIdx = 0; teacherIdx < numTeachers; ++teacherIdx) {
            int totalHours = 0;

            for (size_t timeslotIdx = 0; timeslotIdx < timetable.size(); ++timeslotIdx) {
                if (timetable[timeslotIdx].teacherIdx == static_cast<int>(teacherIdx)) {
                    totalHours++;
                }
            }

            if (totalHours > teachers[teacherIdx].maxHours) {
                totalPenalty++;
            }
        }

        for (size_t timeslotIdx = 0; timeslotIdx < numTimeslots; ++timeslotIdx) {
            int subjectIdx = timetable[timeslotIdx].subjectIdx;
            int teacherIdx = timetable[timeslotIdx].teacherIdx;
            int groupIdx = timetable[timeslotIdx].groupIdx;

            if (!teachers[teacherIdx].subjectKnowledge[subjectIdx]) {
                totalPenalty++;
            }

            if (!groups[groupIdx].subjects[subjectIdx]) {
                totalPenalty++;
            }
        }

        return -totalPenalty;
    }

    std::vector<Timeslot> timetable;
};

class GeneticAlgorithm {
public:
    GeneticAlgorithm() {
        population.resize(populationSize, Schedule());
        generateRandomPopulation();
    }

    void generateRandomPopulation() {
        for (size_t i = 0; i < population.size(); ++i) {
            population[i] = Schedule();
        }
    }

    void evolve() {
        for (int generation = 0; generation < numGenerations; ++generation) {
            std::sort(population.begin(), population.end(), [](const Schedule &a, const Schedule &b) {
                return a.fitness() > b.fitness();
            });

            if (generation % printStep == 0) {
                const Schedule &bestSchedule = population.front();
                std::cout << "Generation " << generation << ": Best Fitness = " << bestSchedule.fitness() << std::endl;
                printSchedule(bestSchedule);
            }

            population.erase(population.begin() + population.size() / 2, population.end());

            while (population.size() < populationSize) {
                Schedule parent1 = tournamentSelection();
                Schedule parent2 = tournamentSelection();
                Schedule child = crossover(parent1, parent2);
                mutate(child);
                population.push_back(child);
            }
        }
        
        std::cout << "Final generation " << numGenerations << ": Best Fitness = " << population.front().fitness() << std::endl;
        printSchedule(population.front());
    }

    Schedule tournamentSelection() const {
        const size_t tournamentSize = 3;

        Schedule bestSchedule = population[rand() % population.size()];

        for (size_t i = 1; i < tournamentSize; ++i) {
            Schedule contender = population[rand() % population.size()];
            if (contender.fitness() > bestSchedule.fitness()) {
                bestSchedule = contender;
            }
        }

        return bestSchedule;
    }

    Schedule crossover(const Schedule &parent1, const Schedule &parent2) const {
        Schedule child;

        size_t point1 = rand() % numTimeslots;
        size_t point2 = rand() % numTimeslots;

        size_t start = std::min(point1, point2);
        size_t end = std::max(point1, point2);

        for (size_t timeslotIdx = 0; timeslotIdx < numTimeslots; ++timeslotIdx) {
            if (timeslotIdx >= start && timeslotIdx <= end) {
                child.timetable[timeslotIdx] = parent1.timetable[timeslotIdx];
            } else {
                child.timetable[timeslotIdx] = parent2.timetable[timeslotIdx];
            }
        }

        return child;
    }

    void mutate(Schedule &schedule) const {
        if (rand() % 100 < 10) {
            size_t timeslotIdx = rand() % numTimeslots;
            size_t propertyToChange = rand() % 3;

            if (propertyToChange == 0) {
                schedule.timetable[timeslotIdx].subjectIdx = rand() % numSubjects;
            } else if (propertyToChange == 1) {
                schedule.timetable[timeslotIdx].teacherIdx = rand() % numTeachers;
            } else {
                schedule.timetable[timeslotIdx].groupIdx = rand() % numGroups;
            }
        }
    }

    void printSchedule(const Schedule &schedule) const {
        std::cout << "Best Schedule:\n";
        for (size_t timeslotIdx = 0; timeslotIdx < schedule.timetable.size(); ++timeslotIdx) {
            int subjectIdx = schedule.timetable[timeslotIdx].subjectIdx;
            int teacherIdx = schedule.timetable[timeslotIdx].teacherIdx;
            int groupIdx = schedule.timetable[timeslotIdx].groupIdx;

            std::cout << "Timeslot: " << timeslotIdx + 1 << ": ";
            std::cout << "Subject: " << subjects[subjectIdx] << ", ";
            std::cout << "Teacher: " << teachers[teacherIdx].name << ", ";
            std::cout << "Group: " << groups[groupIdx].name << std::endl;
        }
    }

    std::vector<Schedule> population;
};

int main() {
    std::srand((unsigned int)std::time(0));

    GeneticAlgorithm geneticAlgorithm;
    geneticAlgorithm.evolve();

    return 0;
}
