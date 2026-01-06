// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <gtest/gtest.h>
#include "core/models/TodoItem.h"

using namespace PersonalCalendar::Core;

class TodoItemTest : public ::testing::Test {
protected:
    TodoItem todo;
};

TEST_F(TodoItemTest, BasicCreation)
{
    todo.uid = QLatin1String("todo-1");
    todo.title = QLatin1String("Buy groceries");
    
    EXPECT_EQ(todo.title, QLatin1String("Buy groceries"));
    EXPECT_TRUE(todo.isValid());
    EXPECT_EQ(todo.status, TodoItem::Status::NeedsAction);
}

TEST_F(TodoItemTest, InvalidWithoutTitle)
{
    todo.uid = QLatin1String("todo-2");
    // title is empty
    
    EXPECT_FALSE(todo.isValid());
}

TEST_F(TodoItemTest, MarkCompleted)
{
    todo.uid = QLatin1String("todo-3");
    todo.title = QLatin1String("Task");
    todo.percentComplete = 0;
    
    EXPECT_FALSE(todo.isCompleted());
    EXPECT_EQ(todo.percentComplete, 0);
    
    todo.markCompleted();
    
    EXPECT_TRUE(todo.isCompleted());
    EXPECT_EQ(todo.percentComplete, 100);
    EXPECT_EQ(todo.status, TodoItem::Status::Completed);
    EXPECT_TRUE(todo.completedDateTime.isValid());
}

TEST_F(TodoItemTest, MarkCancelled)
{
    todo.uid = QLatin1String("todo-4");
    todo.title = QLatin1String("Task");
    
    todo.markCancelled();
    
    EXPECT_EQ(todo.status, TodoItem::Status::Cancelled);
}

TEST_F(TodoItemTest, IsOverdue)
{
    todo.uid = QLatin1String("todo-5");
    todo.title = QLatin1String("Urgent task");
    todo.status = TodoItem::Status::NeedsAction;
    todo.dueDateTime = QDateTime(QDate(2026, 1, 5), QTime(17, 0));
    
    QDateTime now(QDate(2026, 1, 6), QTime(10, 0));
    
    EXPECT_TRUE(todo.isOverdue(now));
    
    // Completed tasks are never overdue
    todo.markCompleted();
    EXPECT_FALSE(todo.isOverdue(now));
}

TEST_F(TodoItemTest, Priorities)
{
    todo.uid = QLatin1String("todo-6");
    todo.title = QLatin1String("High priority task");
    todo.priority = 9;
    
    EXPECT_EQ(todo.priority, 9);
    EXPECT_TRUE(todo.isValid());
}

TEST_F(TodoItemTest, WithCategories)
{
    todo.uid = QLatin1String("todo-7");
    todo.title = QLatin1String("Work task");
    todo.categories.append(QLatin1String("work"));
    todo.categories.append(QLatin1String("important"));
    
    EXPECT_EQ(todo.categories.size(), 2);
    EXPECT_TRUE(todo.categories.contains(QLatin1String("work")));
}

TEST_F(TodoItemTest, ICalSerialization)
{
    todo.uid = QLatin1String("todo-8");
    todo.title = QLatin1String("Test Todo");
    todo.description = QLatin1String("Test Description");
    todo.priority = 5;
    todo.status = TodoItem::Status::InProcess;
    todo.percentComplete = 50;
    
    QString ical = todo.toICalString();
    
    EXPECT_FALSE(ical.isEmpty());
    EXPECT_TRUE(ical.contains(QLatin1String("SUMMARY:Test Todo")));
    EXPECT_TRUE(ical.contains(QLatin1String("DESCRIPTION:Test Description")));
    EXPECT_TRUE(ical.contains(QLatin1String("STATUS:IN-PROCESS")));
    EXPECT_TRUE(ical.contains(QLatin1String("PERCENT-COMPLETE:50")));
    EXPECT_TRUE(ical.contains(QLatin1String("BEGIN:VTODO")));
    EXPECT_TRUE(ical.contains(QLatin1String("END:VTODO")));
}

TEST_F(TodoItemTest, PercentageComplete)
{
    todo.uid = QLatin1String("todo-9");
    todo.title = QLatin1String("Multi-step task");
    
    todo.percentComplete = 0;
    EXPECT_FALSE(todo.isCompleted());
    
    todo.percentComplete = 50;
    EXPECT_FALSE(todo.isCompleted());
    
    todo.percentComplete = 100;
    // Still not completed until status is changed
    EXPECT_FALSE(todo.isCompleted());
    
    todo.markCompleted();
    EXPECT_TRUE(todo.isCompleted());
    EXPECT_EQ(todo.percentComplete, 100);
}

TEST_F(TodoItemTest, Equality)
{
    TodoItem todo1;
    todo1.uid = QLatin1String("todo-10");
    todo1.title = QLatin1String("Task A");
    todo1.status = TodoItem::Status::NeedsAction;
    
    TodoItem todo2;
    todo2.uid = QLatin1String("todo-10");
    todo2.title = QLatin1String("Task A");
    todo2.status = TodoItem::Status::NeedsAction;
    
    EXPECT_TRUE(todo1 == todo2);
    
    todo2.status = TodoItem::Status::Completed;
    EXPECT_FALSE(todo1 == todo2);
}
